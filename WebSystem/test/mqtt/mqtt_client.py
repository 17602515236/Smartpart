import paho.mqtt.client as mqtt

HOST = "127.0.0.1"
PORT = 1883

def test():
    client = mqtt.Client()
    client.connect(HOST, PORT, 60)
    client.publish("chat","hello chenfulin",2)
    client.publish("chat1","hello chenfulin2",2)
    client.loop_forever()

if __name__ == '__main__':
    test()
