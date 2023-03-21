import umbridge
import os


print("Client start.")
port = os.getenv("PORT")
print("Clinet using port:",port)


print(umbridge.supported_models("http://localhost:"+str(port)))

model = umbridge.HTTPModel("http://localhost:"+str(port), "slurm_command")

print(model.get_input_sizes())
print(model.get_output_sizes())

N = 3
config = {}

# print([matrix])
print(model([[1,2,3],[4,5,6]],config))