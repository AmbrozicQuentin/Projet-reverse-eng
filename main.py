from fastapi import FastAPI
from pydantic import BaseModel

app = FastAPI()

class Badge(BaseModel):
    badge_id: int
    badge_name: str
    badge_level: int

# Créer bdd 


# fonction pour créer un badge 
@app.post("/create")
async def create_badge(badge: Badge):
    #bdd.append(badge)
    return {"message": "Badge créé avec succès", "badge": badge}

 
# fonction pour créer un token d'accès 
@app.post("/token")
async def create_token():
    pass

# fonction pour checker l'authentification du badge
@app.get("/check")
async def check():
    pass

# fonction pour modifier les données d'un badge
@app.patch("/patch")
async def modify_token():
    pass
# fonction pour supprimé un badge de la base de donnée 
@app.delete("/delete")
async def delete():
    pass