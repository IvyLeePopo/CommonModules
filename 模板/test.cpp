3. unique_ptr<T, D>������һ��ָ������ΪT�Ķ����unique_ptr����ʹ���Զ���ɾ����D��
4. unique_ptr<T[], D>������һ��ָ������ΪT����������unique_ptr����ʹ���Զ���ɾ����D��
5. reset()���ͷ�unique_ptr��ӵ�еĶ��󲢽�����Ϊnullptr��
6. release()���ͷ�unique_ptr��ӵ�еĶ��󵫲�ɾ������������ָ��ö����ָ�롣
7. operator*()������unique_ptr��ָ��Ķ�������á�
8. operator->()������unique_ptr��ָ��Ķ����ָ�롣
9. operator bool() const���ж�unique_ptr�Ƿ���ж��󣬷���true��false��
10. get()������unique_ptr�����ж����ָ�롣
11. swap()����������unique_ptr��ֵ��
12. deleter()������unique_ptr��ɾ��������