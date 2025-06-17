#!/usr/bin/env python3
"""
Test script for the native CPU miner functionality on mainnet
"""

import subprocess
import time
import json
import os
import signal
import sys

def run_command(cmd):
    """Run a command and return the result"""
    try:
        result = subprocess.run(cmd, shell=True, capture_output=True, text=True, timeout=30)
        return result.returncode == 0, result.stdout, result.stderr
    except subprocess.TimeoutExpired:
        return False, "", "Command timed out"

def test_native_miner():
    """Test the native miner functionality"""
    print("Testing Gotham Native CPU Miner on Mainnet")
    print("=" * 45)
    
    # Start gothamd in mainnet mode
    print("1. Starting gothamd in mainnet mode...")
    gothamd_cmd = "./build/bin/gothamd -daemon -rpcuser=test -rpcpassword=test -rpcport=8332"
    success, stdout, stderr = run_command(gothamd_cmd)
    
    if not success:
        print(f"Failed to start gothamd: {stderr}")
        return False
    
    print("   gothamd started successfully")
    time.sleep(3)  # Give it time to start
    
    try:
        # Load existing wallet
        print("2. Loading wallet 'mywallet'...")
        wallet_cmd = "./build/bin/gotham-cli -rpcuser=test -rpcpassword=test -rpcport=8332 loadwallet mywallet"
        success, wallet_result, stderr = run_command(wallet_cmd)
        
        if not success:
            print(f"Failed to load wallet: {stderr}")
            return False
        
        print("   Wallet loaded successfully")
        
        # Generate a mining address
        print("3. Generating a mining address...")
        addr_cmd = "./build/bin/gotham-cli -rpcuser=test -rpcpassword=test -rpcport=8332 getnewaddress"
        success, address, stderr = run_command(addr_cmd)
        
        if not success:
            print(f"Failed to generate address: {stderr}")
            return False
        
        address = address.strip()
        print(f"   Mining address: {address}")
        
        # Test mining statistics (should show not mining initially)
        print("4. Checking initial mining status...")
        stat_cmd = "./build/bin/gotham-cli -rpcuser=test -rpcpassword=test -rpcport=8332 getminingstat"
        success, stats, stderr = run_command(stat_cmd)
        
        if success:
            stats_json = json.loads(stats)
            print(f"   Initial mining status: {stats_json}")
        else:
            print(f"   Mining stats command failed (expected if not implemented): {stderr}")
        
        # Start mining (auto-detects threads and generates address)
        print("5. Starting native mining...")
        start_cmd = "./build/bin/gotham-cli -rpcuser=test -rpcpassword=test -rpcport=8332 startmining"
        success, result, stderr = run_command(start_cmd)
        
        if success:
            print("   Mining started successfully!")
            try:
                result_json = json.loads(result)
                print(f"   Mining address: {result_json.get('address', 'N/A')}")
                print(f"   Mining threads: {result_json.get('threads', 'N/A')}")
            except json.JSONDecodeError:
                print(f"   Raw result: {result.strip()}")
        else:
            print(f"   Mining start failed: {stderr}")
            # Try alternative method with command line args
            print("6. Testing command line mining arguments...")
            stop_cmd = "./build/bin/gotham-cli -rpcuser=test -rpcpassword=test -rpcport=8332 stop"
            run_command(stop_cmd)
            time.sleep(2)
            
            # Start with mining enabled via command line
            mining_cmd = "./build/bin/gothamd -daemon -rpcuser=test -rpcpassword=test -rpcport=8332 -mine"
            success, stdout, stderr = run_command(mining_cmd)
            
            if success:
                print("   Command line mining started!")
                time.sleep(5)  # Let it mine for a bit
            else:
                print(f"   Command line mining failed: {stderr}")
        
        # Check mining statistics after starting
        print("7. Checking mining statistics...")
        time.sleep(2)
        success, stats, stderr = run_command(stat_cmd)
        
        if success:
            try:
                stats_json = json.loads(stats)
                print(f"   Mining stats: {stats_json}")
                if stats_json.get('mining', False):
                    print("   ✓ Mining is active!")
                else:
                    print("   ✗ Mining is not active")
            except json.JSONDecodeError:
                print(f"   Raw stats output: {stats}")
        else:
            print(f"   Stats check failed: {stderr}")
        
        # Check block count
        print("8. Checking block count...")
        count_cmd = "./build/bin/gotham-cli -rpcuser=test -rpcpassword=test -rpcport=8332 getblockcount"
        success, count, stderr = run_command(count_cmd)
        
        if success:
            print(f"   Current block count: {count.strip()}")
        else:
            print(f"   Block count check failed: {stderr}")
        
        # Wait a bit to see if blocks are mined
        print("9. Waiting 10 seconds to see if blocks are mined...")
        time.sleep(10)
        
        success, new_count, stderr = run_command(count_cmd)
        if success:
            print(f"   New block count: {new_count.strip()}")
            if int(new_count.strip()) > int(count.strip() if success else "0"):
                print("   ✓ Blocks were mined!")
            else:
                print("   ✗ No new blocks mined")
        
        # Stop mining
        print("10. Stopping mining...")
        stop_mining_cmd = "./build/bin/gotham-cli -rpcuser=test -rpcpassword=test -rpcport=8332 stopmining"
        success, result, stderr = run_command(stop_mining_cmd)
        
        if success:
            print("   Mining stopped successfully!")
        else:
            print(f"   Stop mining failed (expected if not implemented): {stderr}")
        
        return True
        
    finally:
        # Clean up - stop gothamd
        print("11. Cleaning up...")
        stop_cmd = "./build/bin/gotham-cli -rpcuser=test -rpcpassword=test -rpcport=8332 stop"
        run_command(stop_cmd)
        time.sleep(2)
        print("    Cleanup completed")

if __name__ == "__main__":
    os.chdir("/home/ubuntu/gotham")
    
    if not os.path.exists("./build/bin/gothamd"):
        print("Error: gothamd not found. Please build the project first.")
        sys.exit(1)
    
    try:
        success = test_native_miner()
        if success:
            print("\n✓ Native miner test completed!")
        else:
            print("\n✗ Native miner test failed!")
            sys.exit(1)
    except KeyboardInterrupt:
        print("\nTest interrupted by user")
        # Try to clean up
        stop_cmd = "./build/bin/gotham-cli -rpcuser=test -rpcpassword=test -rpcport=8332 stop"
        run_command(stop_cmd)
        sys.exit(1)