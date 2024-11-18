import subprocess
import time
import statistics
import argparse

def run_ping(server_address):
    """Run ping command and parse statistics."""
    print(f"Pinging {server_address}...")
    ping_output = subprocess.run(
        ["ping", "-c", "20", server_address],
        capture_output=True,
        text=True
    )
    print(ping_output.stdout)

    # Parse output for min/avg/max/stddev
    stats_line = next(
        (line for line in ping_output.stdout.splitlines() if "min/avg/max/mdev" in line),
        None
    )
    if stats_line:
        stats = stats_line.split("=")[1].strip().split(" ")[0].split("/")
        return {
            "min": float(stats[0]),
            "avg": float(stats[1]),
            "max": float(stats[2]),
            "stddev": float(stats[3])
        }
    else:
        print("Could not parse ping statistics.")
        return None

def run_transfer(client_command, num_runs):
    """Run the client transfer command multiple times and measure stats."""
    transfer_times = []
    for i in range(num_runs):
        print(f"Running transfer {i+1}/{num_runs}...")
        start_time = time.time()
        result = subprocess.run(client_command, capture_output=True, text=True)
        end_time = time.time()

        if result.returncode != 0:
            print(f"Error during transfer {i+1}: {result.stderr}")
            continue

        transfer_time = end_time - start_time
        transfer_times.append(transfer_time)
        print(f"Transfer {i+1} completed in {transfer_time:.4f} seconds.")

    return transfer_times

def calculate_transfer_rates(file_size, transfer_times):
    """Calculate transfer rates from file size and transfer times."""
    rates = [(file_size / t) for t in transfer_times if t > 0]
    return {
        "min": min(rates) if rates else 0,
        "avg": statistics.mean(rates) if rates else 0,
        "max": max(rates) if rates else 0
    }

def main():
    parser = argparse.ArgumentParser(description="Automate client-server testing.")
    parser.add_argument("server_address", help="Server IP address or hostname (e.g., 127.0.0.1 or linux-01.socs.uoguelph.ca).")
    parser.add_argument("port", type=int, help="Server port number.")
    parser.add_argument("file_path", help="Path to the file to transfer.")
    parser.add_argument("--buf_size", type=int, default=4096, help="Buffer size for the client (default: 4096 bytes).")
    parser.add_argument("--num_runs", type=int, default=20, help="Number of test runs (default: 20).")
    args = parser.parse_args()

    client_command = [
        "./client",
        args.file_path,
        f"{args.server_address}:{args.port}",
        str(args.buf_size)
    ]

    # Run ping test
    ping_stats = run_ping(args.server_address)
    if ping_stats:
        print(f"Ping statistics: {ping_stats}")

    # Run file transfer tests
    print("\nStarting file transfer tests...")
    transfer_times = run_transfer(client_command, args.num_runs)
    if not transfer_times:
        print("No successful transfers.")
        return

    # Calculate and display transfer rates
    file_size = os.path.getsize(args.file_path)
    transfer_rates = calculate_transfer_rates(file_size, transfer_times)
    print("\nTransfer Rates (bytes/sec):")
    print(f"Min: {transfer_rates['min']:.2f}, Avg: {transfer_rates['avg']:.2f}, Max: {transfer_rates['max']:.2f}")
    print("\nTest completed.")

if __name__ == "__main__":
    import os
    main()
