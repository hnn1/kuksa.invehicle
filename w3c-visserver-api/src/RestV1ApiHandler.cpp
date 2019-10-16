/*
 * ******************************************************************************
 * Copyright (c) 2019 Robert Bosch GmbH.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v2.0
 * which accompanies this distribution, and is available at
 * https://www.eclipse.org/org/documents/epl-2.0/index.php
 *
 *  Contributors:
 *      Robert Bosch GmbH - initial API and functionality
 * *****************************************************************************
 */

#include "RestV1ApiHandler.hpp"

#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string.hpp>
#include <regex>
#include <limits>

#include "JsonResponses.hpp"
#include "ILogger.hpp"


RestV1ApiHandler::RestV1ApiHandler(std::shared_ptr<ILogger> loggerUtil, std::string& docRoot)
  : logger_(loggerUtil),
    docRoot_(docRoot) {

  // Supported HTTP methods
  regexHttpMethods_ = "^(GET|SET)";

  // Resource strings for REST API hooks. Order must match order in RestV1ApiHandler::Resources enum
  resourceHandleNames_ = std::vector<std::string>{std::string{"signals"}, std::string{"metadata"}};
  // verify that sizes match
  assert(resourceHandleNames_.size() == static_cast<size_t>(Resources::Count));

  // fill regex automatically with all available resources
  regexResources_= std::string("^(");
  for (unsigned i = 0; i < resourceHandleNames_.size() - 1u; ++i) {
    regexResources_ += resourceHandleNames_[i] + std::string("|");
  }
  regexResources_ += resourceHandleNames_[resourceHandleNames_.size() - 1u] + std::string(")");
}

RestV1ApiHandler::~RestV1ApiHandler() {
}

bool RestV1ApiHandler::verifyPathAndStrip(std::string& restTarget, std::string& path) {
  size_t pos = restTarget.find(path);

  // found doc root at correct position
  if (pos == 0) {
    // remove root path to leave only next level
    restTarget.erase(pos, path.length());
    return true;
  }

  return false;
}

// Basic implementation of REST API handling
// For now governed by KISS principle, but in future, it should be re-factored
// to handle independently different methods and resources for easier maintenance..
// possibly by providing hooks for each resource and API version
bool RestV1ApiHandler::GetJson(std::string&& restMethod,
                               std::string&& restTarget,
                               std::string& jsonRequest) {
  bool ret = true;
  std::smatch sm;
  jsoncons::json json;

  // TODO: should client provide request ID when using REST API?
  uint32_t requestId = std::rand() % std::numeric_limits<uint32_t>::max();
  json["requestId"] = requestId;

  // search for supported HTTP requests
  const std::regex regSupportedHttpActions(regexHttpMethods_, std::regex_constants::icase);
  // check REST action method
  std::regex_match (restMethod, sm, regSupportedHttpActions);
  // if supported methods found, parse further
  if (sm.size()) {
    std::string foundStr = sm.str(1);
    boost::algorithm::to_lower(foundStr);

    json["action"] = foundStr;

    if (verifyPathAndStrip(restTarget, docRoot_)) {
       const std::regex regResources(regexResources_, std::regex_constants::icase);

       // get requested resource type
       std::regex_search(restTarget, sm, regResources);
       if (sm.size()) {
         foundStr = sm.str(1);

         if (verifyPathAndStrip(restTarget, foundStr)) {
           if (foundStr == "signals") {
             std::string signalPath;
             std::string delimiter("/");

             if (restTarget.size() && verifyPathAndStrip(restTarget, delimiter)) {
               while (restTarget.size()) {
                 const std::regex regexValidWord("^([A-Za-z]+)");

                 if (std::regex_search(restTarget, sm, regexValidWord)) {
                   foundStr = sm.str(1);
                   signalPath += foundStr;
                   if (verifyPathAndStrip(restTarget, foundStr)) {
                     if ((restTarget.size() == 0)) {
                       break;
                     }
                     else if (verifyPathAndStrip(restTarget, delimiter)) {
                       signalPath += '.';
                     }
                     else {
                       jsonRequest = JsonResponses::malFormedRequest(
                           requestId,
                           json["action"].as_string(),
                           "Signal path delimiter not valid");
                       ret = false;
                     }
                   }
                   else {
                     jsonRequest = JsonResponses::malFormedRequest(
                         requestId,
                         json["action"].as_string(),
                         "Signal path not valid");
                     ret = false;
                   }
                 }
                 else {
                   jsonRequest = JsonResponses::malFormedRequest(
                       requestId,
                       json["action"].as_string(),
                       "Signal path URI not valid");
                   ret = false;
                 }
               }
             }
             else {
               // not supporting retrieving of all signals by default
               jsonRequest = JsonResponses::malFormedRequest(
                   requestId,
                   json["action"].as_string(),
                   "Signals cannot be retrieved in bulk request, only through single signal requests");
               ret = false;
             }

             // update signal path if all is OK
             if (ret) {
               json["path"] = signalPath;
             }
           }
           else if (sm.str(1) == "metadata") {
             // TODO: add support for metadata
             jsonRequest = JsonResponses::noAccess(
                 requestId,
                 json["action"].as_string(),
                 "Access to metadata not yet supported");
           }
         }
         else {
           jsonRequest = JsonResponses::malFormedRequest(
               requestId,
               json["action"].as_string(),
               "Signal path URI not valid");
           ret = false;
         }
       }
       else
       {
         jsonRequest = JsonResponses::malFormedRequest(
             requestId,
             json["action"].as_string(),
             "Requested resource do not exist");
         ret = false;
       }
    }
  }
  else
  {
    // TODO: evaluate what and how we should support HTTP methods (put, patch, ...)
    jsonRequest = JsonResponses::malFormedRequest(
        requestId,
        json["action"].as_string(),
        "Requested HTTP method is not supported");
    ret = false;
  }

  std::stringstream ss;
  ss << pretty_print(json);
  jsonRequest = ss.str();

  return ret;
}
