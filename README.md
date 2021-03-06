# webhook_display

Project that uses the Particle device API to periodically 
wake up, publish to a webhook, then receive updates from 
the webhook that it then displays to a common OLED display.

![Overview Image](/overview.jpg)

## Webhook integration

This embedded code requires a backend to communicate with.
I created a Particle Webook that polls the 
Purple Air API via Thingspeak, with:

- Full URL: `https://api.thingspeak.com/channels/1147330/feeds.json`
- Query Parameters:
```
{
  "api_key": "GMZMBSI7UQNCSX7A",
  "average": "10",
  "results": "1"
}
          
```

NOTE: When you create your own webhook, choose a PurpleAir 
sensor that is closer to you! 

## Creating a Webhook from the PurpleAir API and Thingspeak

You can obtain the keys necessary to access a specific 
PurpleAir sensor directly from the 
[PurpleAir map page](https://www.purpleair.com/map?opt=1/m/i/mPM25/a10/cC0&select=68951#14.72/37.86955/-122.28902).
- Click on the sensor bubble on the map, 
- hover the mouse pointer over the "Get This Widget"
- click on the JSON link

This will open a JSON document that provides:
- `THINGSPEAK_PRIMARY_ID` : This is the channel ID to use in the RESTful "channels" URL above
- `THINGSPEAK_PRIMARY_ID_READ_KEY` : this is the `api_key` needed for the query parameters above.

Enjoy!

