from django.db import models

# Create your models here.
#集中器终端数据表
class Concentrator_cls(models.Model):
        Concentrator_id = models.TextField(default = '',verbose_name = '集中器唯一设备号')
        Lng = models.FloatField(null=True,verbose_name = '经度')
        Lat = models.FloatField(null = True,verbose_name = '维度')
        All_Truck_Space = models.IntegerField(default = 0,verbose_name = '车位总数')
        Free_Truck_Space = models.IntegerField(default = 0,verbose_name = '剩余车位')
        Address = models.TextField(default = '',verbose_name = '地址信息') 
        Price = models.TextField(default = '',verbose_name = '价格信息')
        Motify_DateTime = models.DateTimeField(auto_now=True,verbose_name = '最后修改时间')
        Phone = models.TextField(null=True,verbose_name='联系电话')
        Name = models.TextField(default='',verbose_name='地点名称')
        def __str__(self):
                return self.Name

#传感器终端数据表
class Sensor_cls(models.Model):
	Terminal_id = models.TextField(default='',verbose_name = '传感器唯一设备号')
	Belong_Host = models.ForeignKey(Concentrator_cls,on_delete = models.CASCADE,default = '')
	def __str__(self):
		return self.Terminal_id


