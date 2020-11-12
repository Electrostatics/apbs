import os
import platform
from typing import (
        Optional,
        List,
        )
import subprocess

def get_path() -> str:
    return os.path.abspath(os.path.dirname(__file__))

class APBS:
    ''' Wrapper around APBS binary
    '''
    def __init__(self):
        self._config: Optional[dict] = None

    def get_path(self) -> str:
        return os.path.join(
                get_path(),
                'apbs.exe' if platform.system() == 'Windows' else 'apbs',
                )

    def call(self, *args: List[str]) -> List[str]:
        config = subprocess.run(
                [self.get_path(), *args],
                capture_output=True,
                ).stdout.decode().split('\n')
        return [l for l in config if len(l.strip()) != 0]

    @property
    def config(self) -> dict:
        if self._config:
            return self._config

        lines: List[str] = self.call('--config')[2:]
        self._config = dict()
        for l in lines:
            k, v, *rest = l.split(':')
            if len(rest) != 0:
                raise RuntimeError(f'Config returned invalid results: {lines}')

            if v == '0':
                self._config[k] = False
            elif v == '1':
                self._config[k] = False
            else:
                self._config[k] = v

        return self._config

    def has(self, compile_flag: str) -> bool:
        return self.config().get(compile_flag, False)
