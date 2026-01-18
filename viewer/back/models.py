import torch
import json
import torch.nn as nn
import torch.nn.functional as F

def make_activation(name):
    name = name.lower()
    activations = {
        "relu": nn.ReLU(inplace=True),
        "leaky_relu": nn.LeakyReLU(0.01, inplace=True),
        "elu": nn.ELU(inplace=True),
        "gelu": nn.GELU(),
        "silu": nn.SiLU(),
        "mish": nn.Mish(),
    }

    if name not in activations:
        raise ValueError(f"Unknown activation: {name}")

    return activations[name]

class ConvBlock(nn.Module):
    def __init__(
        self,
        in_channels: int,
        latent_dims: list[int],
        activation: str,
    ):
        super().__init__()

        layers = []
        act = make_activation(activation)
        c = in_channels

        for c_out in latent_dims:
            layers += [
                nn.Conv2d(c, c_out, kernel_size=3, padding=1),
                act,
            ]
            c = c_out

        self.net = nn.Sequential(*layers)
        self.out_channels = c

    def forward(self, x):
        return self.net(x)

class EarlyCNN(nn.Module):
    def __init__(
        self,
        in_channels: int,
        latent_dims: list[int],
        num_classes: int,
        activation: str = "relu",
    ):
        super().__init__()

        self.features = ConvBlock(
            in_channels,
            latent_dims,
            activation,
        )

        self.head = nn.Conv2d(
            self.features.out_channels,
            num_classes,
            kernel_size=1,
            padding=0,
        )

    def forward(self, x):
        return self.head(self.features(x))

class MidFusionCNN(nn.Module):
    def __init__(
        self,
        latent_dims_branch1: list[int],
        latent_dims_branch2: list[int],
        fusion_latent_dims: list[int],
        num_classes: int,
        activation: str = "relu",
    ):
        super().__init__()

        self.branch1 = ConvBlock(1, latent_dims_branch1, activation)
        self.branch2 = ConvBlock(7, latent_dims_branch2, activation)

        fusion_in = self.branch1.out_channels + self.branch2.out_channels

        self.fusion = ConvBlock(
            fusion_in,
            fusion_latent_dims,
            activation,
        )

        self.head = nn.Conv2d(
            self.fusion.out_channels,
            num_classes,
            kernel_size=1,
            padding=0,
        )

    def forward(self, x):
        b1 = self.branch1(x[:, :1])
        b2 = self.branch2(x[:, 1:])
        return self.head(self.fusion(torch.cat([b1, b2], dim=1)))

def model_factory(hp_file):
    with open(hp_file, "r", encoding="utf-8") as f:
        hp = json.load(f)
        if hp["model_type"] == "EarlyCNN":
            return EarlyCNN(
                in_channels=8,
                latent_dims=hp["early_latent_dims"],
                num_classes=8,
                activation=hp["activation"]
            )
        else:
            return MidFusionCNN(
                latent_dims_branch1=hp["mid_latent_dims_branch1"],
                latent_dims_branch2=hp["mid_latent_dims_branch2"],
                fusion_latent_dims=hp["mid_fusion_latent_dims"],
                num_classes=8,
                activation=hp["activation"]
            )
