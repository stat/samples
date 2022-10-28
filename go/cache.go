package cache_client

import (
	"context"
	"crypto/md5"
	"encoding/hex"
	"encoding/json"
	"fmt"
	"log"
	"os"
	"strconv"
	"time"
)

type Client struct {
	Db      *redis_client.Client
	Options *Options
}

type Options struct {
	Redis *redis_client.Options
}

var (
	env_db   int
	env_host string
	env_port string

	instance *Client
)

func init() {
	env_db_string := os.Getenv("CACHE_DB")

	if env_db_string == "" {
		log.Fatalln("CACHE db is empty")
	}

	db, err := strconv.Atoi(env_db_string)

	if err != nil {
		log.Fatalln(err)
	}

	env_db = db
}

func DefaultOptions() *Options {
	options := &Options{
		Redis: redis_client.DefaultOptions(),
	}

	options.Redis.DB = env_db

	return options
}

func Initialize() error {
	return InitializeWithOptions(DefaultOptions())
}

func InitializeWithOptions(options *Options) error {
	if instance != nil {
		return nil
	}

	client, err := NewWithOptions(options)

	if err != nil {
		return err
	}

	instance = client

	return nil
}

func New() (*Client, error) {
	return NewWithOptions(DefaultOptions())
}

func NewWithOptions(options *Options) (*Client, error) {
	db, err := redis_client.NewWithOptions(options.Redis)

	if err != nil {
		return nil, err
	}

	client := &Client{}
	client.Db = db
	client.Options = options

	return client, nil
}

func (self *Client) Sink(key *string, v interface{}) error {
	hash, err := self.KeyHash(key)

	if err != nil {
		return err
	}

	ctx := context.Background()

	cached, err := json.Marshal(v)

	if err != nil {
		return err
	}

	err = self.Db.Set(ctx, *hash, cached, 10*time.Second).Err()

	if err != nil {
		return err
	}

	return nil
}

func (self *Client) Tap(key *string, v interface{}) error {
	hash, err := self.KeyHash(key)

	if err != nil {
		return err
	}

	ctx := context.Background()

	cached, err := self.Db.Get(ctx, *hash).Result()

	if err != nil {
		return err
	}

	err = json.Unmarshal([]byte(cached), v)

	if err != nil {
		return err
	}

	return nil
}

func (self *Client) KeyHash(key *string) (*string, error) {
	if key == nil {
		return nil, fmt.Errorf("key is empty")
	}

	hash := md5.Sum([]byte(*key))
	result := hex.EncodeToString(hash[:])

	return &result, nil
}

func Instance() *Client {
	if instance == nil {
		client, err := New()

		if err != nil {
			log.Fatalln(err)
		}

		instance = client
	}

	return instance
}
