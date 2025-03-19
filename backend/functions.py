import pymongo

myclient = pymongo.MongoClient("mongodb://localhost:27017/")
mydb = myclient["ELET2415"]
mycol = mydb["weather"]

def insert_weather(payload_json):
    mycol.insert_one(payload_json)

def get_weather(start, end):
    try:
        result = mycol.find({"timestamp": {"$gte": start, "$lte": end}}, {"_id": 0})
        return result
    except Exception as e:
        print(f"Mongo Insert Error {e}")
        return False
