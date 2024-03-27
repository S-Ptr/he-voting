import { Component, OnInit } from '@angular/core';
import { FormBuilder } from '@angular/forms';
import { ActivatedRoute, Router } from '@angular/router';
import { VotingService } from '../voting.service';
import SEAL from 'node-seal'
import { SEALLibrary } from 'node-seal/implementation/seal';
import { PlainText } from 'node-seal/implementation/plain-text';
import { CipherText } from 'node-seal/implementation/cipher-text';
import { MatSnackBar } from '@angular/material/snack-bar';
import {ClipboardModule} from '@angular/cdk/clipboard';
var seal:SEALLibrary;
(async () => {
  if (typeof window !== 'undefined') { seal = (await SEAL()); }
  })()

@Component({
  selector: 'app-voter-submitvote',
  templateUrl: './voter-submitvote.component.html',
  styleUrls: ['./voter-submitvote.component.css'],
})
export class VoterSubmitvoteComponent implements OnInit {
  
  title:string = "";
  status:string ="";
  results:number[] = [];
  totalVotes:number = 0;
  percentages:number[] = [];
  candidates:any[] = [];
  chosen:number=-1;
  checkConfirm:boolean=false
  keyarray:string = "";
  serializedPublicKey:string="";
  msg:string = "";
  processing:boolean=false;
  showEncrypted:boolean=false;
  encryptedResult:string="";

  constructor( private route: ActivatedRoute, private votingservice:VotingService, private formBuilder:FormBuilder, private router:Router, private snackbar:MatSnackBar) { 
    
  }

  openSnackBar(message: string, action: string) {
    this.snackbar.open(message, action, {duration:3000});
  }
  
  ngOnInit(): void {
    let id = this.route.snapshot.paramMap.get('identifier')!;
    
    this.votingservice.getPoll(id).subscribe((data:any)=>{
      console.log(data);
      this.title = data.poll.title;
      this.candidates= data.candidates;
      this.status = data.status;
      if(this.status == "active"){
        this.keyarray = this.toBinary(data.poll.secretkey);
        this.serializedPublicKey = data.poll.secretkey;
      }else if(this.status=="finished"){
        this.results = data.result;
        this.encryptedResult = data.encryptedResult;
        this.results.forEach(element => {
          this.totalVotes += element;
          this.percentages.push(0);
        });
      }
      setTimeout(()=>{
      for(let i = 0; i < this.percentages.length;i++){
        this.percentages[i] = 100*this.results[i]/this.totalVotes;
      }
      },50);
    })
  }

  ngAfterContentInit():void{ //for animation purposes
    for(let i = 0; i < this.percentages.length;i++){
      this.percentages[i] = 100*this.results[i]/this.totalVotes;
    }
  }

  toBinary(string:string) {
    const codeUnits = new Uint16Array(string.length);
    for (let i = 0; i < codeUnits.length; i++) {
      codeUnits[i] = string.charCodeAt(i);
    }
    return btoa(String.fromCharCode(...new Uint8Array(codeUnits.buffer)));
  }

  submitVote(){
    if(this.chosen == -1) {
      this.openSnackBar("Molimo vas da napravite izbor", "OK");
      return;
    }
    
    this.processing=true;
    const schemeType = seal.SchemeType.bgv
    const securityLevel = seal.SecurityLevel.tc128
    const polyModulusDegree = 4096
    const bitSize = 31

    const encParms = seal.EncryptionParameters(schemeType)

    // Set the PolyModulusDegree
    encParms.setPolyModulusDegree(polyModulusDegree)

    // Create a suitable set of CoeffModulus primes
    encParms.setCoeffModulus(
      seal.CoeffModulus.Create(polyModulusDegree, new Int32Array([36,36,36]))
    )

    // Set the PlainModulus to a prime of bitSize 32.
    encParms.setPlainModulus(seal.PlainModulus.Batching(polyModulusDegree, bitSize))

    const context = seal.Context(
      encParms, // Encryption Parameters
      true, // ExpandModChain
      securityLevel // Enforce a security level
    )
    
    if (!context.parametersSet()) {
      throw new Error(
        'Could not set the parameters in the given context. Please try different encryption parameters.'
      )
    }
    
    let publickey = seal.PublicKey();
    publickey.load(context, this.serializedPublicKey);

    let dataArray = Array<number>(this.candidates.length).fill(0);

    dataArray[this.chosen-1] = 1;

    let batchEncoder = seal.BatchEncoder(context);

    let plaintext:PlainText = batchEncoder.encode(Int32Array.from(dataArray))!;
    
    let encryptor = seal.Encryptor(context,publickey);

    let encrypted:CipherText;

    if(plaintext){
      console.log(plaintext);
      
      encrypted = encryptor.encrypt(plaintext)!;
    }else{
      this.openSnackBar("Encoding Failure.", "OK");
      return;
    }

    if(encrypted){
      console.log(encrypted);
      let encryptedString = encrypted.save()
    this.votingservice.submitVote(encryptedString,1,Number.parseInt(this.route.snapshot.paramMap.get('identifier')!)).subscribe((data)=>{
      this.processing=false;
      this.openSnackBar("Vas glas je uspesno zabelezen.", "OK");
    });
    }else{
      this.openSnackBar("Encoding Failure.", "OK");
      return;
    }

  }
}
