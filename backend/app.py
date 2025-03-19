import mqtt
import functions
from flask import Flask

app = Flask(__name__)

@app.route("/app/<start>/<end>")
def get_weather(start, end):
    result = functions.get_weather(int(start), int(end))
    return {"data": list(result)}

if __name__ == "__main__":
    mqtt.mqttc.loop_start()
    app.run()

