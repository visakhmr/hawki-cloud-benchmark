from django.db import models

# Create your models here.
class AMI(models.Model):
    ami_id = models.CharField(max_length=10)
