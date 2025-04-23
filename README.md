# AT300 serial control tool for RP2040 (Micropython)

## Flashing (using [uv](https://docs.astral.sh/uv/getting-started/))
```shell
git clone https://github.com/tvt-tech/at300
cd at300
uv run -s flash.py -p /dev/ttyUSB0 -b 115200
```

## Upgrade
```shell
git pull
uv run -s flash.py -p /dev/ttyUSB0 -b 115200
```