/* nyaBot simple discord bot written in C++ using libcurl
    Copyright (C) 2024  Luna
This file is part of nyaBot.

nyaBot is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation; either version 3, or (at your option) any later
version.

nyaBot is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with nyaBot; see the file COPYING3.  If not see
<http://www.gnu.org/licenses/>.  */


#include <curl/curl.h>
#include <curl/easy.h>
#include <curl/websockets.h>
#include <string>
#include <unistd.h>
#include <iostream>
#include "includes/nyaBot.h"

void nyaBot::connect(){
  CURLcode res;
  res = curl_easy_perform(meow);
  if (res == CURLE_OK){
    std::cout << "[*] connected to the websocket succesfully!\n";
  }
  else {
    std::cerr << "[!] something went wrong curl code is " << res << '\n';
    curl_easy_cleanup(meow);
    std::exit(1);
  }
  
}

void nyaBot::sendIdent(){
  std::cout << "[*] sending ident\n";
  std::string ident {R"({"op": 2, "d": {"token": ")" + token + R"(" , "intents": 14, "properties": {"os": "linux", "browser": "meowLib", "device": "meowLib"}}})"};
  size_t sent;
  CURLcode res;
  res = curl_ws_send(meow, ident.c_str(), ident.length(), &sent, 0, CURLWS_TEXT);
  if (res == CURLE_OK){
    std::cout << "[*] ident sent!\n";
  }
  else {
    std::cout << "[!] something went wrong curl code is: " << res << '\n';
  }
}


void nyaBot::sendHeartbeat(){
  srand(std::time(0));
  float random = ((float) rand()) / (float) RAND_MAX;
  sleep(interval * random / 1000);
  while (!stop) {
    std::string heartbeat{R"({"op": 1,"d": )" + std::to_string(sequence) + R"(})"};
    size_t sent;
    CURLcode res;
    res = curl_ws_send(meow, heartbeat.c_str(), heartbeat.length(), &sent, 0, CURLWS_TEXT);
    if (res == CURLE_OK){
      std::cout << "[*] hearbeat sent succesfully!\n";
    }
    else{
      std::cerr << "[!] something went wrong curlcode is: " << res << '\n';
      return;
    }
   sleep(interval / 1000);
  }
}


void nyaBot::getHeartbeatInterval(){
  char buffer[4096];
  size_t rlen;
  const struct curl_ws_frame *nya;
  // receive data from websocket
  try{
    curl_ws_recv(meow, buffer, sizeof(buffer)-1, &rlen, &nya);
    buffer[rlen] = '\0';
    // initialize a json object with the data of buffer
    auto meowJson = nlohmann::json::parse(buffer);
    // create a new json object that has the data of d because discord api sucks
    auto meowNested = meowJson["d"];
    // parse the data of heartbeat_interval into uint64_t and return it
    interval = meowNested["heartbeat_interval"];

  }
  catch(nlohmann::json::exception& e){
    std::cout << "[!] failed to parse buffer\n";
    std::cout << "[!] buffer is " << buffer << '\n';
    std::exit(1);
  }

}

