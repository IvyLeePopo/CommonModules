3. unique_ptr<T, D>：创建一个指向类型为T的对象的unique_ptr，并使用自定义删除器D。
4. unique_ptr<T[], D>：创建一个指向类型为T的数组对象的unique_ptr，并使用自定义删除器D。
5. reset()：释放unique_ptr所拥有的对象并将其置为nullptr。
6. release()：释放unique_ptr所拥有的对象但不删除它，并返回指向该对象的指针。
7. operator*()：返回unique_ptr所指向的对象的引用。
8. operator->()：返回unique_ptr所指向的对象的指针。
9. operator bool() const：判断unique_ptr是否持有对象，返回true或false。
10. get()：返回unique_ptr所持有对象的指针。
11. swap()：交换两个unique_ptr的值。
12. deleter()：返回unique_ptr的删除器对象。