from django.shortcuts import render
from django.http import HttpResponse,JsonResponse
from V1_0.models import Concentrator_cls
import json
import time
# Create your views here.
def index(request):
    Part_Info = Concentrator_cls.objects.all()
    return render(request,'index.html',{'Part_Info':Part_Info.values()})
def WindowInfo_Back(request):
    Lng_val=float(request.GET['Lng'])
    Lat_val=float(request.GET['Lat'])
    Part_Info = Concentrator_cls.objects.all()
    Item = Part_Info.get(Lng=Lng_val,Lat=Lat_val)
    dic = {'Name':Item.Name,'Price':Item.Price,'Address':Item.Address,'Phone':Item.Phone,'F_TS':Item.Free_Truck_Space,'A_TS':Item.All_Truck_Space,'Subscribe_Permit':'是','Motify_Time':Item.Motify_DateTime.strftime('%Y-%m-%d %H:%M:%S')}
    return JsonResponse(json.dumps(dic),safe=False)
