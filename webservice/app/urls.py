from django.urls import path
from . import datacollectviews

urlpatterns = [
    path('dc', datacollectviews.collect_data)
]
