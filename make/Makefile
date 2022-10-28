GO			= go
GO_BUILD		= $(GO) build
GO_CLEAN		= $(GO) clean
GO_TEST			= $(GO) test
GO_GET			= $(GO) get

SRC			:= $(shell find . -type f -name '*.go' -not -path "./vendor/*")

TARGET			= main
BUILDS			= ./build
TESTS			= ./...

BUILD			:= `git rev-parse HEAD`
MAJOR			:= $(shell cat .version | cut -d. -f1)
MINOR			:= $(shell cat .version | cut -d. -f2)
PATCH			:= $(shell cat .version | cut -d. -f3)

GOFLAGS			= -v
LDFLAGS			= 

RELEASEFLAGS := GOOS=linux GOARCH=amd64 CGO_ENABLED=0

SCRIPT_DIR:= $(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))

GCP_PROJECT_ID = ""

DOCKER_IMAGE_BASE_PATH = "asia-south1-docker.pkg.dev/${GCP_PROJECT_ID}"
DOCKER_IMAGE_PATH = "project"
DOCKER_IMAGE_NAME = "api"
DOCKER_IMAGE_TAG = "latest"

.PHONY: clean deploy help run test
.DEFAULT_GOAL := help

$(BUILDS)/$(TARGET): $(SRC)
	@$(GO_BUILD) $(GOFLAGS) -ldflags="$(LDFLAGS)" -o $(BUILDS)/$(TARGET) .
	@echo Binary "(v${MAJOR}.${MINOR}.${PATCH})" built to $(BUILDS)/$(TARGET)!

build: $(BUILDS)/$(TARGET)

clean:
	@$(RM) -r $(BUILDS)

.PHONY: release-development
release-development:
	@$(RELEASEFLAGS) $(GO_BUILD) $(GOFLAGS) -ldflags="$(LDFLAGS)" -o $(BUILDS)/release .
	@echo Binary "(v${MAJOR}.${MINOR}.${PATCH})" built to $(BUILDS)/release!
	@docker build -t "${DOCKER_IMAGE_BASE_PATH}${DOCKER_IMAGE_PATH}/${DOCKER_IMAGE_NAME}-development:${DOCKER_IMAGE_TAG}" -f Dockerfile .
	@docker push "${DOCKER_IMAGE_BASE_PATH}${DOCKER_IMAGE_PATH}/${DOCKER_IMAGE_NAME}-development:${DOCKER_IMAGE_TAG}"

.PHONY: release-production
release-production:
	@$(RELEASEFLAGS) $(GO_BUILD) $(GOFLAGS) -ldflags="$(LDFLAGS)" -o $(BUILDS)/release .
	@echo Binary "(v${MAJOR}.${MINOR}.${PATCH})" built to $(BUILDS)/release!
	@docker build -t "${DOCKER_IMAGE_BASE_PATH}${DOCKER_IMAGE_PATH}-production/api:latest" -f Dockerfile .
	@docker push "${DOCKER_IMAGE_BASE_PATH}${DOCKER_IMAGE_PATH}-production/api:latest"

.PHONY: deploy-development
deploy-development: release-development
	@kubectl apply -f k8s/development/deployment.yaml -n platform-development

.PHONY: restart-development
restart-development:
	@kubectl rollout restart deployment api -n platform-development

.PHONY: deploy-production
deploy-production: release-production
	@kubectl apply -f k8s/production/deployment.yaml -n platform

.PHONY: restart-production
restart-production:
	@kubectl rollout restart deployment api -n platform

.PHONY: run
run: clean build
	API_PORT="7998" \
	$(BUILDS)/$(TARGET)

.PHONY: test
test:
	@$(GO_TEST) -ldflags="$(LDFLAGS)" -v $(TESTS)
