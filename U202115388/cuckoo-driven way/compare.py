""" comparer coockoo hash table with and without infinite loop check """
import os
from cuckoohash import HashTab, HashTabWithoutStrategy

SIZE = 900
EPOPCH = 100
HASHTAB_SIZE = 90


def test_hashtab():
    """ Test the Cuckoo Hash table """
    size = SIZE
    missing = 0
    found = 0

    # create a hash table with an initially small number of bukets
    c = HashTab(HASHTAB_SIZE)

    # Now insert size key/data pairs, where the key is a string consisting
    # of the concatenation of "foobarbaz" and i, and the data is i
    inserted = 0
    for i in range(size):
        if c.insert(str(i)+"foobarbaz", i):
            inserted += 1
    print("There were", inserted, "nodes successfully inserted")

    # Make sure that all key data pairs that we inserted can be found in the
    # hash table. This ensures that resizing the number of buckets didn't
    # cause some key/data pairs to be lost.
    for i in range(size):
        ans = c.find(str(i)+"foobarbaz")
        if ans is None or ans != i:
            print(i, "Couldn't find key", str(i)+"foobarbaz")
            missing += 1

    print("There were", missing, "records missing from Cuckoo")

    # Makes sure that all key data pairs were successfully deleted
    for i in range(size):
        c.delete(str(i)+"foobarbaz")

    for i in range(size):
        ans = c.find(str(i)+"foobarbaz")
        if ans is not None or ans == i:
            print(i, "Couldn't delete key", str(i)+"foobarbaz")
            found += 1
    print("There were", found, "records not deleted from Cuckoo")
    print("loop count: ", c.loop_count)
    return c


def test_hashtab_without_check():
    """ Test the Cuckoo Hash table """
    size = SIZE
    missing = 0
    found = 0

    # create a hash table with an initially small number of bukets
    c = HashTabWithoutStrategy(HASHTAB_SIZE)

    # Now insert size key/data pairs, where the key is a string consisting
    # of the concatenation of "foobarbaz" and i, and the data is i
    inserted = 0
    for i in range(size):
        if c.insert(str(i)+"foobarbaz", i):
            inserted += 1
    print("There were", inserted, "nodes successfully inserted")

    # Make sure that all key data pairs that we inserted can be found in the
    # hash table. This ensures that resizing the number of buckets didn't
    # cause some key/data pairs to be lost.
    for i in range(size):
        ans = c.find(str(i)+"foobarbaz")
        if ans is None or ans != i:
            print(i, "Couldn't find key", str(i)+"foobarbaz")
            missing += 1

    print("There were", missing, "records missing from Cuckoo")

    # Makes sure that all key data pairs were successfully deleted
    for i in range(size):
        c.delete(str(i)+"foobarbaz")

    for i in range(size):
        ans = c.find(str(i)+"foobarbaz")
        if ans is not None or ans == i:
            print(i, "Couldn't delete key", str(i)+"foobarbaz")
            found += 1
    print("There were", found, "records not deleted from Cuckoo")
    print("loop count: ", c.loop_count)
    return c


def __main():
    # 获取当前文件的路径
    current_path = os.path.dirname(os.path.abspath(__file__))
    # 设置txt文件相对于当前文件的路径
    relative_path = f'result/experiment_results_{SIZE}.txt'
    # 使用os.path.join来获取txt文件的完整路径
    filepath = os.path.join(current_path, relative_path)
    print(f"The file path is {filepath}")
    with open(filepath, 'w', encoding='utf-8') as f:
        for _ in range(EPOPCH):
            cuckoohash_withstrategy = test_hashtab()
            # print("finished with strategy")
            cuckoohash_withoutstrategy = test_hashtab_without_check()
            # print("finished without strategy")
            if cuckoohash_withoutstrategy.loop_count > cuckoohash_withstrategy.loop_count:
                if cuckoohash_withoutstrategy.loop_count < 10000:
                    message = (
                        f"{cuckoohash_withstrategy.loop_count} "
                        f"{cuckoohash_withoutstrategy.loop_count}"
                    )
                    f.write(message + '\n')
                else:
                    message = (
                        f"{cuckoohash_withstrategy.loop_count} Infinite"
                    )
                    f.write(message + '\n')
            else:
                f.write(
                    "Cuckoo hash table without infinite loop strategy is better\n")


if __name__ == '__main__':
    __main()
