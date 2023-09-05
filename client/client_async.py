import asyncio
import http.client
import random
import json
import urllib.parse
import signal
import time
from collections import deque

latencies = deque(maxlen=64)

# Define a signal handler to gracefully exit the loop on Ctrl-C
def exit_handler(signum, frame):
    print("Exiting...")
    exit(0)

async def async_send_request(method, path):
    start_time = time.time()
    conn = http.client.HTTPConnection("localhost", 8080)
    conn.request(method, path)
    response = conn.getresponse()
    data = response.read().decode("utf-8")
    conn.close()
    end_time = time.time()
    latency = end_time - start_time
    latencies.append(latency)
    # print(path, json.dumps(data), "->", response.status, latency)
    return data

async def async_send_post_request(path, data):
    start_time = time.time()
    conn = http.client.HTTPConnection("localhost", 8080)
    headers = {"Content-Type": "application/json"}
    conn.request("POST", path, json.dumps(data), headers)  # Serialize data as JSON
    response = conn.getresponse()
    out = response.read().decode("utf-8")
    conn.close()
    end_time = time.time()
    latency = end_time - start_time
    latencies.append(latency)
    # print(path, json.dumps(data), "->", response.status, latency)
    return out

async def async_test_function():

    # Send a GET request to the /movies endpoint
    movies_data = await async_send_request("GET", "/movies")

    # Parse the JSON data
    movies_list = json.loads(movies_data)

    # Select a random movie
    selected_movie = random.choice(movies_list)
    data = {"movie": selected_movie}
    theaters = await async_send_post_request("/find", data)

    theaters_list = json.loads(theaters)
    selected_theater = random.choice(theaters_list)

    array_length = random.randint(0, 6)  # Adjust the range as needed
    random_integers = [random.randint(0, 19) for _ in range(array_length)]

    data = {"theater": selected_theater, "movie": selected_movie, "seats": random_integers}
    book = await async_send_post_request("/seats", data)    

    
    data = {"theater": selected_theater, "movie": selected_movie}
    seats = await async_send_post_request("/bookings", data)

    mean_latency = sum(latencies) / len(latencies)
    print(f"Mean latency of the last 64 test operations: {mean_latency:.8f} seconds")

    
# Set the signal handler for Ctrl-C
signal.signal(signal.SIGINT, exit_handler)

async def main():
    num_clients = 10  # Number of concurrent clients
    tasks = [async_test_function() for _ in range(num_clients)]
    await asyncio.gather(*tasks)

    print("Enter loop!!")
    while True:  # Infinite loop
        #time.sleep(0.1)
        num_clients = random.randint(1,50)  # Number of concurrent clients
        tasks = [async_test_function() for _ in range(num_clients)]
        await asyncio.gather(*tasks)
    #        await async_test_function()

loop = asyncio.get_event_loop()
try:
    loop.run_until_complete(main())
except KeyboardInterrupt:
    pass
finally:
    loop.close()
