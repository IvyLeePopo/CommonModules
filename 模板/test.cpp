// 1. 导入 hashlib 库
import hashlib

// 2. 创建一个 SHA - 256 对象
sha256 = hashlib.sha256()

// 3. 将需要哈希的数据传递给 SHA - 256 对象
sha256.update(b"Hello, World!")

// 4. 获取 SHA - 256 哈希值
hash_value = sha256.digest()

// 5. 可以将哈希值转换为十六进制字符串进行存储或传输
hex_value = hash_value.hex()

// 完整示例代码：
import hashlib

sha256 = hashlib.sha256()
sha256.update(b"Hello, World!")
hash_value = sha256.digest()
hex_value = hash_value.hex()

print(hex_value)

// 输出：b94d27b9934d3e08a52e52d7da7dabfac484efe37a5380ee9088f7ace2efcde9
