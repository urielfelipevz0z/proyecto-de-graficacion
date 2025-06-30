#!/bin/bash

# Script to generate all 9 reference images for Proyecto 2
# This modifies the configuration constants and recompiles for each combination

cd /home/beladen/Laboratorio-de-Graficación/proyecto-2

echo "Generating 9 reference images for Proyecto 2 - Direction Sampling..."

# Array of sampling methods with proper enum names
method_enums=("UNIFORM_SPHERE" "UNIFORM_HEMISPHERE" "COSINE_HEMISPHERE")
method_names=("uniformsphere" "uniformhemi" "cosinehemi")

# Array of samples per pixel
samples=(32 512 2048)

for i in "${!method_enums[@]}"; do
    method_enum=${method_enums[$i]}
    method_name=${method_names[$i]}
    
    for spp in "${samples[@]}"; do
        echo "Generating image-${method_name}${spp}.ppm..."
        
        # Update the configuration in rt.cpp
        sed -i "s/const SamplingMethod SAMPLING_METHOD = [^;]*;/const SamplingMethod SAMPLING_METHOD = $method_enum;/" rt.cpp
        sed -i "s/const int SAMPLES_PER_PIXEL = [^;]*;/const int SAMPLES_PER_PIXEL = $spp;/" rt.cpp
        
        # Recompile
        g++ -O3 -fopenmp rt.cpp -o rt
        
        if [ $? -eq 0 ]; then
            # Run the ray tracer
            echo "Rendering with ${method_name}, ${spp} samples per pixel..."
            time ./rt
            
            # Rename the output image
            mv image.ppm image-${method_name}${spp}.ppm
            echo "Generated: image-${method_name}${spp}.ppm"
        else
            echo "Compilation failed for ${method_name} ${spp}!"
            exit 1
        fi
        
        echo "---"
    done
done

echo "All 9 images generated successfully!"
echo "Images generated:"
ls -la image-*.ppm
