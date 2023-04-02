import umbridge
import subprocess
import os
import sys
import concurrent.futures

def test_model(model_name):
    model = umbridge.HTTPModel(url, model_name)
    model.get

    print("input_sizes:", model.get_input_sizes())
    print("output_sizes:", model.get_output_sizes())

    config = {}

    print("evaluation:", model([[1.01],], config))


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

model_names = ["forward", "backward", "inward", "outward"]
with concurrent.futures.ThreadPoolExecutor() as executor:
    results = executor.map(test_model, model_names)
