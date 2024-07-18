import os
import pandas as pd
import matplotlib.pyplot as plt
import argparse

def get_most_recent_file(directory, extension=".txt"):
    files = [f for f in os.listdir(directory) if f.endswith(extension)]
    if not files:
        raise FileNotFoundError(f"No files with extension '{extension}' found in the directory.")
    files = [os.path.join(directory, f) for f in files]
    most_recent_file = max(files, key=os.path.getctime)
    return most_recent_file

def plot_data(file_path, aggregation_factor=10):
    data = pd.read_csv(file_path, sep='\t')    
    
    data['time'] = data['time'].astype(int) // aggregation_factor * aggregation_factor
    data = data.groupby('time').mean().reset_index()
        
    x = data['time']
    num_plots = len(data.columns) - 1
    
    fig, axes = plt.subplots(num_plots, 1, figsize=(8, 6 * num_plots), sharex=True)
    
    for i, column in enumerate(data.columns[1:]):
        ax = axes[i] if num_plots > 1 else axes
        ax.plot(x, data[column], marker='', linestyle='-', linewidth=1.5, label=column)
        ax.set_ylabel('Y-axis label')
        ax.set_title(f'Plot for {column}')
        ax.grid(True)
        ax.legend()    
    
    for ax in axes:
        ax.set_xlim(x.min(), x.max())
        ax.set_ylim(data.iloc[:, 1:].values.min(), data.iloc[:, 1:].values.max())
    
    plt.xlabel('X-axis label')
    plt.tight_layout()
    plt.show()

def main(directory, specific_file=None, aggregation_factor=10):
    if specific_file:
        file_path = specific_file
    else:
        file_path = get_most_recent_file(directory)
    print(f"Using file: {file_path}")
    plot_data(file_path, aggregation_factor=aggregation_factor)

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Plot data from a .txt file.")
    parser.add_argument('directory', type=str, help='Directory containing .txt files')
    parser.add_argument('--file', type=str, help='Specific file to use (optional)', default=None)
    parser.add_argument('--aggregation', type=int, help='Aggregation factor for downsampling (default: 10)', default=10)
    
    args = parser.parse_args()
    main(args.directory, args.file, aggregation_factor=args.aggregation)
