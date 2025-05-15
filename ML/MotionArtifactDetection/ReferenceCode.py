from sklearn.model_selection import train_test_split
from sklearn.utils import shuffle
import torch
import torchvision
import torchvision.transforms as transforms
import pickle
import numpy as np
import matplotlib.pyplot as plt
import os

def init_weights(m):
    if type(m) == torch.nn.Conv3d or type(m) == torch.nn.Linear:
        torch.nn.init.xavier_uniform_(m.weight)
        m.bias.data.fill_(0.01)

class my3DConvNet(torch.nn.Module):

    def __init__(self, img_shape,num_channels, num_classes):
        x, y, z = img_shape
        c = num_channels
        k = num_classes

        print('in_shape',(c,x,y,z))
        super(my3DConvNet, self).__init__()
        self.group1 = torch.nn.Sequential(
            torch.nn.Conv3d(c, 32, kernel_size=3, stride=1, padding=1),
            torch.nn.BatchNorm3d(32),
            torch.nn.LeakyReLU(),
            torch.nn.MaxPool3d(kernel_size=2, stride=2),
            # torch.nn.Dropout(0.5)
        )
        self.group2 = torch.nn.Sequential(
            torch.nn.Conv3d(32, 64, kernel_size=3, stride=1, padding=1),
            torch.nn.BatchNorm3d(64),
            torch.nn.LeakyReLU(),
            torch.nn.MaxPool3d(kernel_size=2, stride=2),
            # torch.nn.Dropout(0.5)
        )
        self.fc1 = torch.nn.Sequential(
            torch.nn.Linear(64 * (x//4) * (y//4) * (z//4), 256),
            torch.nn.LeakyReLU(),
            torch.nn.Dropout(0.5)
        )
        self.fc2 = torch.nn.Sequential(
            torch.nn.Linear(256, 256),
            torch.nn.LeakyReLU(),
            torch.nn.Dropout(0.5)
        )
        self.fc3 = torch.nn.Sequential(
            torch.nn.Linear(256, k)
        )

    def forward(self, x):
        x = self.group1(x)
        x = self.group2(x)
        x = x.view(x.size(0), -1)
        x = self.fc1(x)
        x = self.fc2(x)
        return self.fc3(x)
