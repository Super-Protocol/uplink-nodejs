# Go parameters
GOCMD=go
GOBUILD=$(GOCMD) build
GOCLEAN=$(GOCMD) clean
GOTEST=$(GOCMD) test
GOGET=$(GOCMD) get
# Uplink-c
GIT_REPO=https://github.com/Super-Protocol/uplink-c-sp.git
UPLINKC_NAME=uplink-c-sp
UPLINKC_VERSION=v1.7.0-sp
#Library Name
LIBRARY_NAME_POSIX=libuplinkcv1.7.0-sp.a
LIBRARY_NAME_WIN=libuplinkcv1.7.0-sp.dll
LIBRARY_UPLINK=*.h
DELETE_LIBRARY_HEADER=libuplinkcv1.7.0-sp.h
#Library Path
MACOS_DYLIB_DEFAULT_PAYH=/usr/local/lib
#JSFOLDER
JSFOLDER=dist
#Color
RED_COLOR=\033[31m
GREEN_COLOR=\033[32m
RESET_COLOR=\033[0m
#
build:
ifdef OS
	git clone -b ${UPLINKC_VERSION} ${GIT_REPO}
	(cd ${UPLINKC_NAME}) && (${GOBUILD} -o ../${LIBRARY_NAME_WIN} -buildmode=c-shared) && (move ${LIBRARY_UPLINK} ../)
	rmdir /s ${UPLINKC_NAME}
else
	echo "$(shell uname)";\
  mkdir -p ./$(JSFOLDER);\
  if [ ! -d $(UPLINKC_NAME) ]; then\
    git clone -b $(UPLINKC_VERSION) $(GIT_REPO);\
  fi;\
  cd $(UPLINKC_NAME);\
  $(GOBUILD) -o ../$(LIBRARY_NAME_POSIX) -buildmode=c-archive;\
  cp $(LIBRARY_UPLINK) ../;\
  cd ../;\
  cp error.js ./$(JSFOLDER);\
  if test -d ./$(UPLINKC_NAME); then rm -rf ./$(UPLINKC_NAME); fi;\
  echo ' $(GREEN_COLOR) \n Successfully build $(RESET_COLOR)';
endif

clean:
ifdef OS
	(IF EXIST ${LIBRARY_UPLINK}; (del "${LIBRARY_UPLINK}")) && (IF EXIST ${DELETE_LIBRARY_HEADER}; (del "${DELETE_LIBRARY_HEADER}")) && (IF EXIST ${LIBRARY_NAME_WIN}; (del "${LIBRARY_NAME_WIN}")) && (IF EXIST ${UPLINKC_NAME}; (rmdir /s "${UPLINKC_NAME}"))
else
	if test -d $(UPLINKC_NAME); then rm -rf $(UPLINKC_NAME); fi
	if test -f ./$(LIBRARY_UPLINK); then rm ./$(LIBRARY_UPLINK); fi;\
  if test -f ./$(DELETE_LIBRARY_HEADER); then rm ./$(DELETE_LIBRARY_HEADER); fi;\
    if [ $(shell uname) = Darwin ]; then\
      if test -f ./$(LIBRARY_NAME_DARWIN); then rm ./$(LIBRARY_NAME_DARWIN); fi;\
     fi;\
     if [ $(shell uname) = Linux ]; then\
      if test -f $(LIBRARY_NAME_LINUX); then rm $(LIBRARY_NAME_LINUX); fi;\
     fi;
endif
	@echo ' $(GREEN_COLOR) \n Successfully cleaned $(RESET_COLOR)';
