Import("env")
import os

# Đường dẫn tới file .env ở thư mục gốc
env_file = os.path.join(env.get("PROJECT_DIR"), ".env")

if os.path.exists(env_file):
    print("--> Đang tự động đọc file .env nạp vào C++ Compiler...")
    with open(env_file, "r") as f:
        for line in f:
            line = line.strip()
            if line and not line.startswith("#") and "=" in line:
                key, val = line.split("=", 1)
                key = key.strip()
                val = val.strip()
                # Thêm định nghĩa -DKEY="VAL" vào GCC Compiler Flags
                env.Append(CPPDEFINES=[(key, val)])
else:
    print("--> Không tìm thấy file .env, sử dụng giá trị mặc định trong config.h")
