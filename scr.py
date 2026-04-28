import statistics

def analyze_data(file_path):
    try:
        # 1. Read values from the .txt file
        with open(file_path, 'r') as f:
            # Convert each line to a float, ignoring empty lines
            values = [float(line.strip()) for line in f if line.strip()]

        if len(values) < 2:
            print("Error: Need at least two numbers to calculate differences.")
            return

        # 2. Find the difference between each line
        differences = []
        print(f"{'Comparison':<25} | {'Difference':<10}")
        print("-" * 40)
        
        for i in range(1, len(values)):
            diff = values[i] - values[i-1]
            differences.append(diff)
            # Printing every difference might be a lot for long files, 
            # so we just print them out here:
            print(f"Line {i+1:3} - Line {i:3}      | {diff:.6f}")

        # 3. Calculate Average and Variance of the differences
        avg_diff = statistics.mean(differences)
        var_diff = statistics.variance(differences)

        # 4. Print Summary Results
        print("-" * 40)
        print(f"Total Numbers Processed: {len(values)}")
        print(f"Total Differences:       {len(differences)}")
        print(f"AVERAGE DIFFERENCE:      {avg_diff:.10f}")
        print(f"VARIANCE OF DIFFS:       {var_diff:.10f}")

    except FileNotFoundError:
        print(f"Error: The file '{file_path}' was not found.")
    except ValueError:
        print("Error: Ensure the file contains only numbers.")

# Run the script (replace 'your_data_file.txt' with your actual filename)
analyze_data('your_data_file.txt')
