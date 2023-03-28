import umbridge
import os,sys


print("Client start.")

# Get port
try:
    port = os.getenv("PORT")
except:
    port = 4242
if(port == None):
    port = 4242
print("Client is using port:",port)

# Get host
if len(sys.argv) > 1:
    host = sys.argv[1]
else:
    host="localhost"
url = "http://"+host+":"+str(port)
print("Connecting to server at:", url)


print(umbridge.supported_models(url))
model = umbridge.HTTPModel(url, "slurm_LB")

print(model.get_input_sizes())
print(model.get_output_sizes())

config = {}

print(model([[100],],config))

for i in range(100,104):
    print(model([[i],],config))