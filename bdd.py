from typing import Annotated

from fastapi import Depends, FastAPI, HTTPException, Query
from sqlmodel import Field, Session, SQLModel, create_engine, select


class BadgeBase(SQLModel):
    name: str = Field(index=True)
    level: int | None = Field(default=None, index=True)


class Badge(BadgeBase, table=True):
    id: int | None = Field(default=None, primary_key=True)
    secret_name: str


class BadgePublic(BadgeBase):
    id: int


class BadgeCreate(BadgeBase):
    secret_name: str


class BadgeUpdate(BadgeBase):
    name: str | None = None
    level: int | None = None
    secret_name: str | None = None


sqlite_file_name = "database.db"
sqlite_url = f"sqlite:///{sqlite_file_name}"

connect_args = {"check_same_thread": False}
engine = create_engine(sqlite_url, connect_args=connect_args)


def create_db_and_tables():
    SQLModel.metadata.create_all(engine)


def get_session():
    with Session(engine) as session:
        yield session


SessionDep = Annotated[Session, Depends(get_session)]
app = FastAPI()


@app.on_event("startup")
def on_startup():
    create_db_and_tables()


@app.post("/create/", response_model=BadgePublic)
def create_badge(badge: BadgeCreate, session: SessionDep):
    db_badge = Badge.model_validate(badge)
    session.add(db_badge)
    session.commit()
    session.refresh(db_badge)
    return db_badge


@app.get("/read/", response_model=list[BadgePublic])
def read_badges(
    session: SessionDep,
    offset: int = 0,
    limit: Annotated[int, Query(le=100)] = 100,
):
    badges = session.exec(select(Badge).offset(offset).limit(limit)).all()
    return badges


@app.get("/read/{badge_id}", response_model=BadgePublic)
def read_badge(badge_id: int, session: SessionDep):
    badge = session.get(Badge, badge_id)
    if not badge:
        raise HTTPException(status_code=404, detail="Badge not found")
    return badge


@app.patch("/update/{badge_id}", response_model=BadgePublic)
def update_badge(badge_id: int, badge: BadgeUpdate, session: SessionDep):
    db_badge = session.get(Badge, badge_id)
    if not db_badge:
        raise HTTPException(status_code=404, detail="Badge not found")
    badge_data = badge.model_dump(exclude_unset=True)
    db_badge.sqlmodel_update(badge_data)
    session.add(db_badge)
    session.commit()
    session.refresh(db_badge)
    return db_badge


@app.delete("/delete/{badge_id}")
def delete_badge(badge_id: int, session: SessionDep):
    badge = session.get(Badge, badge_id)
    if not badge:
        raise HTTPException(status_code=404, detail="Badge not found")
    session.delete(badge)
    session.commit()
    return {"ok": True}