from typing import Dict, List
from fastapi import FastAPI
from fastapi_pagination import Page, add_pagination, paginate
from pydantic import BaseModel, Field

import os, json

class PackageRepository(BaseModel):
    url: str = Field(..., example="https://www.github.com/")
    type: str = Field(..., example="git")
class PackageStats(BaseModel):
    stars: int = Field(..., example=0)
    downloads: int = Field(..., example=0)
    views: int = Field(..., example=0)
class Package(BaseModel):
    name: str = Field(..., example="Steve")
    author: str = Field(..., example="Steve")
    repository: PackageRepository
    stats: PackageStats
    dependencies: Dict[str, str]
    dependants: List[str]

app = FastAPI()
packages = []
for x in os.listdir("./cdn/packages/"):
    pkg = json.load(open(f"./cdn/packages/{x}/package.json"))
    packages.append(pkg)

@app.get("/")
async def index():
    return "OK"

@app.get("/pkgs", response_model=Page[Package])
async def list_packages():
    return paginate(packages)

add_pagination(app)

# app.run(debug=True, host="0.0.0.0", port=80)
