#include "../include/slashCommandInt.h"
#include "../meowHttp/src/includes/https.h"
#include "../include/log.h"
#include <cstdint>
#include <nlohmann/json.hpp>
#include <print>
#include <nlohmann/json_fwd.hpp>

SlashCommandInt::SlashCommandInt(
  const std::string_view id,
  const std::string_view token,
  const std::string_view commandNamee,
  uint64_t userId,
  const std::string& applicationId
) : commandName{commandNamee},
    userId{userId},
    interactionId{id},
    interactionToken{token},
    applicationId(applicationId){}

void SlashCommandInt::respond(const std::string_view response, int flags) {
  nlohmann::json j;
  j["type"] = 4;
  j["data"] = nlohmann::json::object();
  if(flags != 0) j["data"]["flags"] = flags;
  j["data"]["content"] = response;
  auto handle = meowHttp::Https()
    .setUrl("https://discord.com/api/v10/interactions/" + interactionId + '/' + interactionToken + "/callback")
    .setHeader("content-type: application/json")
    .setPostfields(j.dump());
  if(handle.perform() != OK) {
    Log::Log("failed to respond to an interaction\n");
  }
}

void SlashCommandInt::respond(){
  nlohmann::json j;
  j["type"] = 5;
  auto handle = meowHttp::Https()
    .setUrl("https://discord.com/api/v10/interactions/" + interactionId + '/' + interactionToken + "/callback")
    .setHeader("content-type: application/json")
    .setPostfields(j.dump());
  if(handle.perform() != OK) {
    Log::Log("failed to respond to an interaction");
  }
}


void SlashCommandInt::manualResponse(const nlohmann::json& j){
  std::string a;
  auto handle = meowHttp::Https()
    .setUrl("https://discord.com/api/v10/interactions/" + interactionId + '/' + interactionToken + "/callback")
    .setHeader("content-type: application/json")
    .setWriteData(&a)
    .setPostfields(j.dump());
  if(handle.perform() != OK && handle.getLastStatusCode() != 204){
    Log::Log("failed to respond to an interaction");
    std::println("{}", a);
  }
}


void SlashCommandInt::manualEdit(const nlohmann::json& j){
  std::string a;
  auto handle = meowHttp::Https()
    .setUrl("https://discord.com/api/v10/webhooks/" + applicationId  + '/' + interactionToken + "/messages/@original")
    .setHeader("content-type: application/json")
    .setCustomMethod("PATCH")
    .setWriteData(&a)
    .setPostfields(j.dump());
  if(handle.perform() != OK && handle.getLastStatusCode() != 204){
    Log::Log("failed to respond" + a);
  }

}


void SlashCommandInt::edit(const std::string_view response, int flags){
  nlohmann::json j;
  if(flags != 0) j["flags"] = flags;
  j["content"] = response;
  std::string a;
  auto handle = meowHttp::Https()
    .setUrl("https://discord.com/api/v10/webhooks/" + applicationId  + '/' + interactionToken + "/messages/@original")
    .setCustomMethod("PATCH")
    .setHeader("content-type: application/json")
    .setWriteData(&a)
    .setPostfields(j.dump());
  if(handle.perform() != OK) {
    Log::Log("something went wrong while responding\n" + a);
  }
}

