import subprocess
import unittest


class cuckooTest(unittest.TestCase):
    def __init__(self, methodName: str = "runTest") -> None:
        super().__init__(methodName)
        self.cmd = ["bin/cuckoo-hash", "-t", "500000", "-l", "20", "-T" ,"20"]

    def test_cuckoo(self):
        return
        resps = []
        hash_num = [2, 3, 4, 8]
        for i in hash_num:
            cmd = self.cmd.copy() + ["-n", str(i)]
            resp = subprocess.run(cmd, capture_output=True, text=True)
            ## get the last line of the output
            with open(f"log_cuckoo_{i}.txt", "w") as f:
                # 添加换行符
               f.write(resp.stdout)
            
            lines = resp.stdout.split("\n")
            last_line = lines[-2]
            rate = float(last_line.split(":")[1].split("%")[0])
            print(rate)
            resps.append(rate)
        print(resps)

    def test_cuckoo2(self):
        return
        resps = []
        blocked_num = [2, 3, 4, 8]
        for i in blocked_num:
            cmd = self.cmd.copy() + ["-n", "2", "-c", "blocked_cuckoo", "-b", str(i)]
            resp = subprocess.run(cmd, capture_output=True, text=True)
            ## get the last line of the output
            with open(f"log_blocked_cuckoo_{i}.txt", "w") as f:
                # 添加换行符
                f.write(resp.stdout)
            
            lines = resp.stdout.split("\n")
            last_line = lines[-2]
            rate = float(last_line.split(":")[1].split("%")[0])
            resps.append(rate)
        print(resps)

    def test_cuckoo3(self):
        return 
        resps = []
        stash_num = [10,50,100,200]
        for i in stash_num:
            cmd = self.cmd.copy() + ["-n", "2", "-c", "stash_cuckoo", "-s", str(i)]
            resp = subprocess.run(cmd, capture_output=True, text=True)
            ## get the last line of the output
            with open(f"log_stash_cuckoo_{i}.txt", "w") as f:
                # 添加换行符
                f.write(resp.stdout)
            
            lines = resp.stdout.split("\n")
            last_line = lines[-2]
            with open("log.txt", "w") as f:
                # 添加换行符
                for line in lines[:-2]:
                    f.write(line + "\n")
            rate = float(last_line.split(":")[1].split("%")[0])
            print(rate)
            resps.append(rate)
        print(resps)

    def test_cuckoo4(self):
        resps = []
        blocked_num = [2, 3, 4, 8]
        stash_num = [100, 200, 400, 800]
        for i in blocked_num:
            l = []
            for j in stash_num:
                cmd = self.cmd.copy() + [
                    "-n",
                    "2",
                    "-c",
                    "stash_blocked_cuckoo",
                    "-b",
                    str(i),
                    "-s",
                    str(j),
                ]
                resp = subprocess.run(cmd, capture_output=True, text=True)
                with open(f"log_stash_blocked_cuckoo_{i}_{j}.txt", "w") as f:
                    # 添加换行符
                    f.write(resp.stdout)
                ## get the last line of the output
                lines = resp.stdout.split("\n")
                last_line = lines[-2]
                rate = float(last_line.split(":")[1].split("%")[0])
                print(rate)
                l.append(rate)
            resps.append(l)
        print(resps)


if __name__ == "__main__":
    unittest.main()
