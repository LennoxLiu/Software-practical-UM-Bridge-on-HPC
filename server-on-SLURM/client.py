import umbridge
import numpy as np

print(umbridge.supported_models("http://localhost:4242"))

model = umbridge.HTTPModel("http://localhost:4242", "slurm_command")

print(model.get_input_sizes())
print(model.get_output_sizes())

N = 3
config = {}

# print([matrix])
print(model([[1.1,2,3],[4,5,6]],config)) 
# You have to enter [matrix,[]] instead of [matrix] 
# or it will say "InvalidInput: Number of inputs does not match number of model inputs. Expected 1 but got 1" 