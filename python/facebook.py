import os

import facebook as FB

from lib.resource import Resource

class Facebook(Resource):
  access_token = None
  client = None
  context = None
  cid = None

  ##
  # Errors
  ##

  ERR_01 = "Facebook access_token is required!"
  ERR_02 = "Facebook object_id is required!"
  ERR_03 = "Facebook access_token and object_id mismatch! access_token should be a system token bound to the page."

  ##
  # Fields
  ##

  # Templates

  LIMIT = ",".join([
    "limit(1000)"
  ])

  FIELDS_FOR_ALL = ",".join([
    "id",
    "created_time",
    "updated_time"
  ])

  FIELDS_FOR_ENGAGEMENTS = ','.join([
    "comments.limit(0).summary(1)",
    "likes.limit(0).summary(1)",
    "reactions.limit(0).summary(1)",
    "shares"
  ])

  # Objects

  FIELDS_FOR_VIDEO = ",".join([
    FIELDS_FOR_ALL,
    FIELDS_FOR_ENGAGEMENTS,

    # Attributes
    "description",
    "embeddable",
    "format",
    "is_crosspost_video",
    "is_crossposting_eligible",
    "length",
    "permalink_url",
    "picture",
    "published",
    "content_category",
    "content_tags",
    "is_episode",
    "is_instagram_eligible",
    "status",
    "source",
    "title",

    # Edges
    "captions{create_time,is_auto_generated,is_default,locale,locale_name,uri}",
    "thumbnails{id,name,scale,uri,is_preferred,height,width}",
    "video_insights"
  ])

  # Collections

  FIELDS_FOR_FEED = ",".join([
    FIELDS_FOR_ALL,
    FIELDS_FOR_ENGAGEMENTS,
    "caption",
    "description",
    "link",
    "message",
    "object_id",
    "source"
    "title",
    "type",
    "parent_id",
    "permalink_url",
  ])

  FIELDS_FOR_VIDEOS = ",".join([
    FIELDS_FOR_VIDEO
  ])

  ##
  # Init
  ##

  # Assumes:
  #   - access_token is a page_access_token
  #   - object_id is the page_id

  def __init__(self, access_token, object_id):
    super()

    if access_token is None:
        raise Exception(self.ERR_01)

    if object_id is None:
        raise Exception(self.ERR_02)

    self.access_token = access_token
    self.client = FB.GraphAPI(access_token=access_token, version="3.1")

    self.context = self.me()
    self.cid = self.context["id"]

    if object_id != self.cid:
      raise Exception(self.ERR_03)

  def request_object(self, id=None, fields=""):
    if id is None:
      id = self.cid

    return self.client.get_object(id=id, fields=fields)

  def request_edge(self, edge, id=None, fields=""):
    if id is None:
      id = self.cid

    return self.client.get_connections(id=id, connection_name=edge, fields=fields)

  def request_edges(self, edge, id=None, fields=""):
    if id is None:
      id = self.cid

    return self.client.get_all_connections(id=id, connection_name=edge, fields=fields)

  def me(self):
    return self.request_object("me")

  def feed(self):
    return self.request_edges("feed", fields=self.FIELDS_FOR_FEED)

  def videos(self):
    return self.request_edges("videos", fields=self.FIELDS_FOR_VIDEOS)

  def extract(self, response):
    return response["data"]

  def type(self):
    return "etl-facebook"

  def name(self):
    return self.cid
