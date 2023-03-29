import umbridge
import numpy as np

print(umbridge.supported_models("http://localhost:4242"))

model = umbridge.HTTPModel("http://localhost:4242", "get_eigenvector_eigenvalue")

print(model.get_input_sizes())
print(model.get_output_sizes())

N = 10
config = {"steps": 1000}

# Generate diag matrix
matrix = np.zeros(N*N)
for i in range(N):
    matrix[i+i*N] = i
# print(matrix)

print(matrix.reshape(N,N))

# print([matrix])
print(model([list(matrix),[]],config)) 
# You have to enter [matrix,[]] instead of [matrix] 
# or it will say "InvalidInput: Number of inputs does not match number of model inputs. Expected 1 but got 1" 