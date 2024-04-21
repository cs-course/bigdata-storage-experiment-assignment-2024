# 实验名称 Lab3 观测分析性能

  使用getput测试工具，编写测试脚本，测试了本机部署的OpenStack Swift存储性能。

# 实验环境

  本机环境如下：

   处理器 AMD Ryzen 7 7840H with Radeon 780M Graphics 3.80 GHz
   机带RAM 32.0 GB (27.7 GB 可用)
   系统类型 64 位操作系统, 基于 x64 的处理器
   操作系统 Windows 11 家庭中文版


# 实验记录

## 测试上传7个不同尺寸的对象，对应的不同的延迟，并且统计平均延迟和范围。这7个对象的大小分别为1KB、10KB、100KB、1MB、10MB、100MB、1GB。测试数据见截图。测试脚本见gpsuite.conf。

# 实验小结

  使用了getput这个benchmark tool，并且编写gpsuite脚本，很方便地使用进行多次不同的测试。