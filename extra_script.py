Import("env")
import os

# Đường dẫn tới file .env ở thư mục gốc
env_file = os.path.join(env.get("PROJECT_DIR"), ".env")

if os.path.exists(env_file):
    print("--> Đang tự động đọc file .env nạp vào C++ Compiler...")
    config = {}
    with open(env_file, "r", encoding="utf-8") as f:
        for line in f:
            line = line.strip()
            if line and not line.startswith("#") and "=" in line:
                key, val = line.split("=", 1)
                key = key.strip()
                val = val.strip().strip('"').strip("'")
                config[key] = val

    # Giải quyết biến tham chiếu (Variable Interpolation)
    for k, v in list(config.items()):
        for ref_k, ref_v in config.items():
            v = v.replace(f"${{{ref_k}}}", ref_v).replace(f"${ref_k}", ref_v)
        if v in config:
            v = config[v]
        config[k] = v
        # Thêm định nghĩa -DKEY="VAL" vào GCC Compiler Flags
        env.Append(CPPDEFINES=[(k, v)])
else:
    print("--> Không tìm thấy file .env, sử dụng giá trị mặc định trong config.h")
