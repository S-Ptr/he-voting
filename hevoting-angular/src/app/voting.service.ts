import { HttpClient,  HttpHeaders } from '@angular/common/http';
import { Injectable } from '@angular/core';

@Injectable({
  providedIn: 'root'
})
export class VotingService {

  constructor(private http:HttpClient) { }

  uri="http://localhost:3000"


  createPoll(data:any){

    //Set the Content-Type header to application/json
    const headers = new HttpHeaders({ 'Content-Type': 'application/json' });

    console.log(data)
    return this.http.post(`${this.uri}/create-poll`, data, { headers: headers });
  }

  getPoll(id:string){
    return this.http.get(`${this.uri}/poll/`+id);
  }

  submitVote(encrypted:any, user:any, poll:any){

    const data ={
      user:user,
      poll:poll,
      vote:encrypted
    }
    console.log(data);
    const headers = new HttpHeaders({ 'Content-Type': 'application/json' });

    return this.http.post(`${this.uri}/submit-vote`, data, { headers: headers });
  }
}
