# Generated by Django 2.1.5 on 2019-02-20 17:26

from django.db import migrations, models


class Migration(migrations.Migration):

    dependencies = [
        ('V1_0', '0005_auto_20190220_2339'),
    ]

    operations = [
        migrations.AddField(
            model_name='concentrator_cls',
            name='Name',
            field=models.TextField(default='', verbose_name='地点名称'),
        ),
        migrations.AddField(
            model_name='concentrator_cls',
            name='Pnone',
            field=models.TextField(null=True, verbose_name='联系电话'),
        ),
    ]
