import umbridge
import numpy as np

print(umbridge.supported_models("http://localhost:4242"))

model = umbridge.HTTPModel("http://localhost:4242", "slurm_command")

print(model.get_input_sizes())
print(model.get_output_sizes())

N = 3
config = {}

# print([matrix])
print(model([[1,2,3],[4,5,6]],config))