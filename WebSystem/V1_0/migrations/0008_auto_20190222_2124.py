# Generated by Django 2.1.5 on 2019-02-22 13:24

from django.db import migrations, models


class Migration(migrations.Migration):

    dependencies = [
        ('V1_0', '0007_auto_20190221_0131'),
    ]

    operations = [
        migrations.AddField(
            model_name='concentrator_cls',
            name='Motify_Date',
            field=models.DateField(auto_now=True, verbose_name='最后修改日期'),
        ),
        migrations.AlterField(
            model_name='concentrator_cls',
            name='Motify_Time',
            field=models.TimeField(auto_now=True, verbose_name='最后修改时间'),
        ),
    ]