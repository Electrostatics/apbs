import os
def get_path() -> str:
    return os.path.abspath(os.path.dirname(__file__))
