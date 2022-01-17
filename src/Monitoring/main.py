import time
import rticonnextdds_connector as rti
from time import sleep
from influxdb_client import InfluxDBClient, Point
from influxdb_client.client.write_api import SYNCHRONOUS

client = InfluxDBClient.from_config_file("config.ini")
write_api = client.write_api(write_options=SYNCHRONOUS)

with rti.open_connector(url="./CPS.xml", config_name="ACCParticipantLibrary::Monitoring") as connector:
    inputA = connector.get_input("Subscriber::SpeedReader")
    inputB = connector.get_input("Subscriber::DistanceReader")

    while True:
        print("Submitting values..")
        sleep(1)
        inputA.take()
        for sample in inputA.samples:
            if sample.valid_data:
                point = Point("Speed")
                point.field("payload", int(sample["payload"]))
                write_api.write(bucket="cps", record=point)
        inputB.take()
        for sample in inputB.samples:
            if sample.valid_data:
                point = Point("Distance")
                point.field("payload", float(sample["payload"]))
                write_api.write(bucket="cps", record=point)
