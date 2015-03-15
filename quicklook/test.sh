#!/bin/sh -e

QL=qlmanage
QLG=Pud.qlgenerator

sudo rm -rv /Library/QuickLook/$QLG
sudo cp -vr $QLG /Library/QuickLook

$QL -r
$QL -p ./cibola.pud

