// 1. ���� hashlib ��
import hashlib

// 2. ����һ�� SHA - 256 ����
sha256 = hashlib.sha256()

// 3. ����Ҫ��ϣ�����ݴ��ݸ� SHA - 256 ����
sha256.update(b"Hello, World!")

// 4. ��ȡ SHA - 256 ��ϣֵ
hash_value = sha256.digest()

// 5. ���Խ���ϣֵת��Ϊʮ�������ַ������д洢����
hex_value = hash_value.hex()

// ����ʾ�����룺
import hashlib

sha256 = hashlib.sha256()
sha256.update(b"Hello, World!")
hash_value = sha256.digest()
hex_value = hash_value.hex()

print(hex_value)

// �����b94d27b9934d3e08a52e52d7da7dabfac484efe37a5380ee9088f7ace2efcde9
