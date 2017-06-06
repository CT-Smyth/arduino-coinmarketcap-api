#include "CoinMarketCapApi.h"

CoinMarketCapApi::CoinMarketCapApi(Client &client)	{
  this->client = &client;
}

String CoinMarketCapApi::SendGetToCoinMarketCap(String command) {
  String headers="";
  String body="";
  bool finishedHeaders = false;
  bool currentLineIsBlank = true;
	long now;
	bool avail;
	// Connect with google-maps api over ssl
	if (client->connect(COINMARKETCAP_HOST, COINMARKETCAP_SSL_PORT)) {
		// Serial.println(".... connected to server");
		String a="";
		char c;
		int ch_count=0;
		client->println("GET " + command + " HTTP/1.1");
    client->println("Host: " COINMARKETCAP_HOST);
		client->println("User-Agent: arduino/1.0.0");
		client->println("");
		now=millis();
		avail=false;
		while (millis()-now<1500) {
			while (client->available()) {
				char c = client->read();
				//Serial.write(c);

        if(!finishedHeaders){
          if (currentLineIsBlank && c == '\n') {
            finishedHeaders = true;
          }
          else{
            headers = headers + c;

          }
        } else {
          body=body+c;
          ch_count++;
        }

        if (c == '\n') {
          currentLineIsBlank = true;
        }else if (c != '\r') {
          currentLineIsBlank = false;
        }

				avail=true;
			}
			if (avail) {
				Serial.println("Body:");
				Serial.println(body);
				Serial.println("END");
				break;
			}
		}
	}

  return body;
}

CMCTickerResponse CoinMarketCapApi::GetTickerInfo(String coinId, String currency) {
  // https://api.coinmarketcap.com/v1/ticker/bitcoin/
  String command="/v1/ticker/" + coinId + "/";
  if (currency != "") {
    command = command + "?currency=" + currency;
  }

  String response = SendGetToCoinMarketCap(command);
  CMCTickerResponse responseObject = CMCTickerResponse();
  DynamicJsonBuffer jsonBuffer;
	JsonObject& root = jsonBuffer.parseObject(response);
  if (root.success()) {
		if (root.containsKey("error")) {
			 responseObject.error = root["error"].as<String>();
		} else {
        responseObject.id = root["id"].as<String>();
        responseObject.name = root["name"].as<String>();
        responseObject.symbol = root["symbol"].as<String>();
        responseObject.rank = root["rank"].as<int>();
        responseObject.price_usd = root["price_usd"].as<float>();
//       id: "bitcoin",
// name: "Bitcoin",
// symbol: "BTC",
// rank: "1",
// price_usd: "2598.37",
// price_btc: "1.0",
// 24h_volume_usd: "1201740000.0",
// market_cap_usd: "42545027009.0",
// available_supply: "16373737.0",
// total_supply: "16373737.0",
// percent_change_1h: "-0.19",
// percent_change_24h: "3.3",
// percent_change_7d: "16.69",
// last_updated: "1496681052"
    }
  } else {
    responseObject.error = "Failed to parse JSON";
  }

  return responseObject;

}
