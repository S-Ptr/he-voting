import { Component, OnInit } from '@angular/core';
import { FormBuilder } from '@angular/forms';
import { ActivatedRoute, Router } from '@angular/router';
import { VotingService } from '../voting.service';
import SEAL from 'node-seal'
import { SEALLibrary } from 'node-seal/implementation/seal';
import { PlainText } from 'node-seal/implementation/plain-text';
import { CipherText } from 'node-seal/implementation/cipher-text';
var seal:SEALLibrary;
(async () => {
  if (typeof window !== 'undefined') { seal = (await SEAL()); }
  })()

@Component({
  selector: 'app-voter-submitvote',
  templateUrl: './voter-submitvote.component.html',
  styleUrls: ['./voter-submitvote.component.css']
})
export class VoterSubmitvoteComponent implements OnInit {
  
  title:string = "";
  candidates:any[] = [];
  chosen:number=-1;
  checkConfirm:boolean=false
  keyarray:string = "";
  serializedPublicKey:string="";
  msg:string = "";

  constructor( private route: ActivatedRoute, private votingservice:VotingService, private formBuilder:FormBuilder, private router:Router) { }
  
  ngOnInit(): void {
    let id = this.route.snapshot.paramMap.get('identifier')!;
    this.votingservice.getPoll(id).subscribe((data:any)=>{
      this.title = data.poll.title;
      this.candidates= data.candidates;
      this.keyarray = this.toBinary(data.poll.secretkey);
      this.serializedPublicKey = data.poll.secretkey;
    })

  


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
      this.msg = "Molimo vas izaberite"
      return;
    }
    const schemeType = seal.SchemeType.bgv
    const securityLevel = seal.SecurityLevel.tc128
    const polyModulusDegree = 4096
    const bitSize = 32

    const encParms = seal.EncryptionParameters(schemeType)

    // Set the PolyModulusDegree
    encParms.setPolyModulusDegree(polyModulusDegree)

    // Create a suitable set of CoeffModulus primes
    encParms.setCoeffModulus(
      seal.CoeffModulus.BFVDefault(polyModulusDegree, securityLevel)
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

    let evaluator = seal.Evaluator(context);

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
      this.msg = "Encoding failure. Commit self-destruction."
      return;
    }

    if(encrypted){
      console.log(encrypted);
      let encryptedString = encrypted.save()
    this.votingservice.submitVote(encryptedString,"user",this.route.snapshot.paramMap.get('identifier')!).subscribe((data)=>{
      this.msg = "Sve ok, proveri bekend.";
    });
    }else{
      this.msg = "Encryption failure. Commit self-destruction."
      return;
    }

  }
}
