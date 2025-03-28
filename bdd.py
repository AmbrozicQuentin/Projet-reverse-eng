from typing import Annotated
from fastapi import Depends, FastAPI, HTTPException, Query
from sqlmodel import Field, Session, SQLModel, create_engine, select
from contextlib import asynccontextmanager

class Badge(SQLModel, table=True):
    badge_id: int | None = Field(default=None, primary_key=True)
    badge_name: str = Field(index= True)
    badge_level: int | None = Field(default=None, index=True)
    
sqllite_file_name = "database.db"
sqlite_url = f"sqlite:///{sqllite_file_name}"

connect_args = {"check_same_thread": False}
engine = create_engine(sqlite_url, connect_args=connect_args)

def create_db_and_tables():
    SQLModel.metadata.create_all(engine)

def get_session():
    with Session(engine) as session:
        yield session

SessionDep = Annotated[Session, Depends(get_session)]

app = FastAPI()

@asynccontextmanager
async def lifespan(app: FastAPI):
    # Code pour le démarrage de l'application
    create_db_and_tables()
    yield  # Point où l'application démarre, elle continue à tourner ici
    # Code pour l'arrêt de l'application (si nécessaire)
    # Exemple : fermer des connexions, etc.

app = FastAPI(lifespan=lifespan)

@app.post("/create")
async def create_badge(badge: Badge, session: SessionDep) -> Badge:
    session.add(badge)
    session.commit()
    session.refresh(badge)
    return badge

@app.get("/check/{badge_id}")
async def check(badge_id: int, session: SessionDep) -> Badge:
    badge = session.get(Badge, badge_id)
    if not badge:
        raise HTTPException(status_code=404, detail="Badge not found")
    return badge

@app.delete("/delete/{badge_id}")
async def delete_badge(badge_id: int, session: SessionDep):
    badge = session.get(Badge, badge_id)
    if not badge:
        raise HTTPException(status_code=404, detail="Badge not found")  # Correction ici
    session.delete(badge)
    session.commit()
    return {"ok": True}
