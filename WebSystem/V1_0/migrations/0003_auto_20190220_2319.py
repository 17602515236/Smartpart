# Generated by Django 2.1.5 on 2019-02-20 15:19

from django.db import migrations, models


class Migration(migrations.Migration):

    dependencies = [
        ('V1_0', '0002_auto_20190219_1411'),
    ]

    operations = [
        migrations.AddField(
            model_name='concentrator_cls',
            name='Address',
            field=models.TextField(default='', verbose_name='地址信息'),
        ),
        migrations.AddField(
            model_name='concentrator_cls',
            name='All_Truck_Space',
            field=models.IntegerField(default=0, verbose_name='车位总数'),
        ),
        migrations.AddField(
            model_name='concentrator_cls',
            name='Free_Truck_Space',
            field=models.IntegerField(default=0, verbose_name='剩余车位'),
        ),
        migrations.AddField(
            model_name='concentrator_cls',
            name='Lat',
            field=models.FloatField(null=True, verbose_name='维度'),
        ),
        migrations.AddField(
            model_name='concentrator_cls',
            name='Lng',
            field=models.FloatField(null=True, verbose_name='经度'),
        ),
        migrations.AddField(
            model_name='concentrator_cls',
            name='Motify_Time',
            field=models.DateField(auto_now=True, verbose_name='最后修改时间'),
        ),
        migrations.AddField(
            model_name='concentrator_cls',
            name='Price',
            field=models.TextField(default='', verbose_name='价格信息'),
        ),
    ]