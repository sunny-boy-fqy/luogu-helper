#!/usr/bin/env python3
import re
import sys
import os

def expand_includes(file_path, processed_files=None):
    if processed_files is None:
        processed_files = set()
    
    abs_path = os.path.abspath(file_path)
    if abs_path in processed_files:
        return ""
    processed_files.add(abs_path)
    
    if not os.path.exists(file_path):
        print(f"// Error: {file_path} not found", file=sys.stderr)
        return f'#include "{file_path}" // File not found'

    output = []
    with open(file_path, 'r', encoding='utf-8') as f:
        for line in f:
            # Match #include "..."
            match = re.match(r'^\s*#include\s*"(.*)"', line)
            if match:
                include_path = match.group(1)
                # Resolve relative path based on the current file's directory
                current_dir = os.path.dirname(file_path)
                
                # Search paths
                search_paths = [
                    os.path.join(current_dir, include_path),
                    os.path.join("/home/fangqiyu/program/include", include_path)
                ]
                
                full_include_path = None
                for p in search_paths:
                    if os.path.exists(p):
                        full_include_path = p
                        break
                
                # Check if it's one of our project headers
                if full_include_path:
                    output.append(f"// --- Start of {include_path} ---")
                    output.append(expand_includes(full_include_path, processed_files))
                    output.append(f"// --- End of {include_path} ---")
                else:
                    output.append(line.rstrip())
            else:
                # Basic pragma once removal to avoid issues in bundled code
                if not line.strip().startswith("#pragma once"):
                    output.append(line.rstrip())
    
    return "\n".join(output)

def main():
    input_file = "main.cpp"
    if len(sys.argv) > 1:
        input_file = sys.argv[1]
    if not os.path.exists(input_file):
        print(f"Error: {input_file} not found")
        sys.exit(1)
    
    bundled_code = expand_includes(input_file)
    
    # Try to copy to clipboard
    try:
        import subprocess
        if sys.platform == "linux":
            # Try xclip then xsel
            copied = False
            # Check for xclip
            try:
                p = subprocess.Popen(["xclip", "-selection", "clipboard"], stdin=subprocess.PIPE, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
                p.communicate(input=bundled_code.encode('utf-8'))
                if p.returncode == 0:
                    print("✅ Bundled code copied to clipboard (using xclip)")
                    copied = True
            except FileNotFoundError:
                pass
            
            if not copied:
                # Check for xsel
                try:
                    p = subprocess.Popen(["xsel", "-bi"], stdin=subprocess.PIPE, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
                    p.communicate(input=bundled_code.encode('utf-8'))
                    if p.returncode == 0:
                        print("✅ Bundled code copied to clipboard (using xsel)")
                        copied = True
                except FileNotFoundError:
                    pass
            
            if copied:
                return
        elif sys.platform == "darwin":
            p = subprocess.Popen(["pbcopy"], stdin=subprocess.PIPE)
            p.communicate(input=bundled_code.encode('utf-8'))
            print("✅ Bundled code copied to clipboard (macOS)")
            return
        elif sys.platform == "win32":
            p = subprocess.Popen(["clip"], stdin=subprocess.PIPE)
            p.communicate(input=bundled_code.encode('utf-8'))
            print("✅ Bundled code copied to clipboard (Windows)")
            return
    except Exception as e:
        print(f"Warning: Could not copy to clipboard: {e}")

    # Fallback: write to file
    with open("bundled_code.cpp", "w", encoding='utf-8') as f:
        f.write(bundled_code)
    print("✅ Bundled code saved to bundled_code.cpp")

if __name__ == "__main__":
    main()