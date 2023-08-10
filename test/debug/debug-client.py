import umbridge
import subprocess
import os
import sys
import concurrent.futures
import time

def evaluate_model(model_name):
    model = umbridge.HTTPModel(url, model_name)

    input_sizes = model.get_input_sizes()
    output_sizes = model.get_output_sizes()

    config = {}
    # Pass request time
    result = model([[time.time()]], config)

    return model_name, input_sizes, output_sizes, result

def output_results(results):
    for model_name, input_sizes, output_sizes, result in results:
        id, request_time, start, end, duration = result[0]

        print(f"Model {model_name}: {id=}")
        print(f"  Input sizes: {input_sizes}")
        print(f"  Output sizes: {output_sizes}")
        print(f"  Request sent: {time.ctime(request_time)}")
        print(f"  Request received: {time.ctime(start)}")
        print(f"  Request finished: {time.ctime(end)}")
        print(f"  Duration: {duration}s")

print("Client start.")

# Get port
try:
    port = os.getenv("PORT")
except:
    port = 4242
if (port == None):
    port = 4242
print("Client is using port:", port)

# Get host
if len(sys.argv) > 1:
    host = sys.argv[1]
else:
    # Define the bash command you want to run
    bash_command = "hostname"

    # Run the bash command using subprocess
    process = subprocess.Popen(bash_command.split(), stdout=subprocess.PIPE)
    output, error = process.communicate()

    # Print the output of the bash command
    # print(output.decode())
    host = output.decode()[:-1]
url = "http://"+host+":"+str(port)
print("Connecting to server at:", url)
print("supported_models:", umbridge.supported_models(url))

#Test different models in parallel
model_names = ["forward", "backward", "inward", "outward"]
print(f"Testing parallel requests to different models {model_names}")

with concurrent.futures.ThreadPoolExecutor() as executor:
   results = executor.map(evaluate_model, model_names)

output_results(results)

#Test multiple parallel request to a single LoadBalancer instance
model_name = "backward"
num_requests = 5

model_names = [model_name] * num_requests
print(f"Testing {num_requests} parallel requests to model {model_name}")

with concurrent.futures.ThreadPoolExecutor() as executor:
    results = executor.map(evaluate_model, model_names)

output_results(results)
